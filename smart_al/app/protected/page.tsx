import { redirect } from "next/navigation";
import { createClient } from "@/lib/supabase/server";
import { InfoIcon } from "lucide-react";
import { Suspense } from "react";

async function UserDetails() {
  const supabase = await createClient();
  const { data, error } = await supabase.auth.getClaims();

  if (error || !data?.claims) {
    redirect("/auth/login");
  }
  // 2. Check if user profile exists in 'user_data' table
  const { data: profile, error: profileError } = await supabase
    .from("user_data")
    .select("id, username")
    .eq("user_id", data.claims.sub) // user id from auth 
    .maybeSingle();

    if (profileError) {
      console.error("Profile fetch error:", profileError);
    }

    if (!profile){
      const usernameFromMetadata = data.claims.user_metadata?.username;
      const { error: insertError } = await supabase.from("user_data").insert({
        user_id: data.claims.sub,
        username: usernameFromMetadata
      });
      if (insertError) {
        console.error("Profile insert error:", insertError);
      }
    }


  return JSON.stringify(data.claims, null, 2);


}
export default function ProtectedPage() {
  return (
    <div className="flex-1 w-full flex flex-col gap-12">
      <div className="w-full">
        <div className="bg-accent text-sm p-3 px-5 rounded-md text-foreground flex gap-3 items-center">
          <InfoIcon size="16" strokeWidth={2} />
          This is a protected page that you can only see as an authenticated
          user
        </div>
      </div>
      <div className="flex flex-col gap-2 items-start">
        <h2 className="font-bold text-2xl mb-4">Your user details</h2>
        <pre className="text-xs font-mono p-3 rounded border max-h-32 overflow-auto">
          <Suspense>
            <UserDetails />
          </Suspense>
        </pre>
      </div>
    </div>
  );
}
