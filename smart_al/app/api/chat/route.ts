import { createClient } from "@/lib/supabase/server";
import { google } from "@ai-sdk/google";
import { generateText } from "ai";
import { NextResponse } from "next/server";

type ChatRole = "user" | "assistant";

type ChatMessage = {
  role: ChatRole;
  content: string;
};

type MeasurementRow = {
  bac: number | null;
  created_at: string | null;
};

const MAX_MESSAGES = 12;
const MAX_MEASUREMENTS = 5;

const BASE_SYSTEM_PROMPT = `
You are a health assistant for Smart-Al users.
Give general health and wellness advice using the user's profile context when helpful.
Use the latest BAC measurements as additional context when relevant.
Never claim to be a doctor, never diagnose diseases, and never prescribe medication.
If the user reports severe or emergency symptoms, tell them to contact local emergency services immediately.
Keep responses practical, clear, and short.
`.trim();

function sanitizeMessages(input: unknown): ChatMessage[] {
  if (!Array.isArray(input)) return [];

  return input
    .filter((item): item is Record<string, unknown> => typeof item === "object" && item !== null)
    .map((item) => {
      const role = item.role;
      const content = item.content;
      if (
        (role !== "user" && role !== "assistant") ||
        typeof content !== "string" || !content.trim()
      ) {
        return null;
      }
      return {
        role,
        content: content.trim(),
      };
    })
    .filter((message): message is ChatMessage => message !== null)
    .slice(-MAX_MESSAGES);
}

function formatUserProfile(profile: {
  name: string | null;
  age: number | null;
  weight: number | null;
  height: number | null;
  gender: string | null;
}) {
  return [
    `Name: ${profile.name ?? "unknown"}`,
    `Age: ${profile.age ?? "unknown"}`,
    `Weight: ${profile.weight ?? "unknown"}`,
    `Height: ${profile.height ?? "unknown"}`,
    `Gender: ${profile.gender ?? "unknown"}`,
  ].join("\n");
}
// latest results in string format to be included in the system prompt for the AI model
function formatLatestMeasurements(measurements: MeasurementRow[] | null) {
  if (!measurements?.length) {
    return "No recent BAC measurements found.";
  }
 // Format each measurement with its BAC value and timestamp, and join them into a single string
  return measurements
    .map((measurement, index) => {
      const timestamp = measurement.created_at
        ? new Date(measurement.created_at).toISOString()
        : "unknown-time";
      const bac = measurement.bac ?? "unknown";
      return `${index + 1}. BAC: ${bac}, Timestamp: ${timestamp}`;
    })
    .join("\n");
}

export async function POST(request: Request) {
  if (!process.env.GOOGLE_GENERATIVE_AI_API_KEY) {
    return NextResponse.json(
      {
        error: "Missing GOOGLE_GENERATIVE_AI_API_KEY.",
      },
      { status: 500 },
    );
  }
 // supabase client creation and user authentication
  try {
    const supabase = await createClient();
    const {
      data: { user },
      error: userError,
    } = await supabase.auth.getUser();

    if (userError || !user) {
      return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
    }

    const body = (await request.json()) as { messages?: unknown }; // user input messages from the request body
    const messages = sanitizeMessages(body.messages);

    if (!messages.length) {
      return NextResponse.json({ error: "At least one user message is required." }, { status: 400 });
    }

    if (!messages.some((message) => message.role === "user")) {
      return NextResponse.json({ error: "A user message is required." }, { status: 400 });
    }
      // load the user's profile data 
    const { data: profileData, error: profileError } = await supabase
      .from("profiles")
      .select("name, age, weight, height, gender")
      .eq("id", user.id)
      .maybeSingle();

    if (profileError) {
      return NextResponse.json(
        { error: "Failed to load your profile context for chat." },
        { status: 500 },
      );
    }
    // format the user profile data into a string that can be included in the system prompt for the AI model
    const profileContext = formatUserProfile({
      name: profileData?.name ?? null,
      age: profileData?.age ?? null,
      weight: profileData?.weight ?? null,
      height: profileData?.height ?? null,
      gender: profileData?.gender ?? null,
    });
    // Measurements history (max 5 latest)
    const { data: measurementData, error: measurementError } = await supabase
      .from("measurements_with_username")
      .select("bac, created_at")
      .eq("user_id", user.id)
      .order("created_at", { ascending: false })
      .limit(MAX_MEASUREMENTS);

    if (measurementError) {
      console.error("Failed to load measurements for chat context:", measurementError.message);
    }

    const measurementContext = formatLatestMeasurements(
      (measurementData as MeasurementRow[] | null) ?? null,
    );
    // inject the user profile and latest measurements into the system prompt for the AI model
    const systemPrompt = `${BASE_SYSTEM_PROMPT}

User profile data:
${profileContext}

Latest BAC measurements:
${measurementContext}`;
    const { text } = await generateText({
      model: google("gemma-3-27b-it"), // ai model 
      system: systemPrompt,
      messages,
    });

    return NextResponse.json({ reply: text }); // returns the response 
  } catch (error) {
    const message = error instanceof Error ? error.message : "Failed to generate chat response.";
    return NextResponse.json({ error: message }, { status: 500 });
  }
}
