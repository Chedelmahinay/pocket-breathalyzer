import { redirect } from "next/navigation";
import { createClient } from "@/lib/supabase/server";

export default async function UserQueryCard() {
  const supabase = await createClient();

  const { data: authData, error: authError } = await supabase.auth.getUser();
  if (authError || !authData?.user) redirect("/auth/login");

  const user = authData.user;

  const { data: profile, error: profileError } = await supabase
    .from("user_data")
    .select("username, created_at")
    .eq("user_id", user.id) 
    .maybeSingle();

  return (
    <div className="rounded-2xl border p-5 space-y-2">
      <div className="text-sm text-muted-foreground">Current user</div>

      <div>
        <span className="font-medium">Email:</span> {user.email}
      </div>

      {profileError ? (
        <div className="text-sm text-red-600">
          Fetching profile failed: {profileError.message}
        </div>
      ) : (
        <>
          <div>
            <span className="font-medium">Username:</span>{" "}
            {profile?.username ?? "Not set"}
          </div>

          <div className="text-sm text-muted-foreground">
            Profile created at:{" "}
            {profile?.created_at ? new Date(profile.created_at).toLocaleString() : "-"}
          </div>
        </>
      )}
    </div>
  );
}
