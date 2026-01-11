import { redirect } from "next/navigation";
import { createClient } from "@/lib/supabase/server";


export default async function UserPage() {
 
  const supabase = await createClient();

  // Hae kirjautunut käyttäjä 
  const { data: authData, error: authError } = await supabase.auth.getUser();
  if (authError || !authData?.user) {
    redirect("/auth/login"); 
  }


  
  const user = authData.user;

  
  const { data: profile, error: profileError } = await supabase
    .from("user_data")
    .select("username, created_at")
    .maybeSingle();

  return (
    <div className="mx-auto max-w-2xl p-6 space-y-6">
      <h1 className="text-2xl font-semibold">User Info</h1>

      <div className="rounded-2xl border p-5 space-y-2">
        <div className="text-sm text-muted-foreground">Current user</div>
        <div><span className="font-medium">Email:</span> {user.email}</div>
        {profileError ? (
          <div className="text-sm text-red-600">
            Fetcing profile failed: {profileError.message}
          </div>
        ) : (
          <>

            <div>
              <span className="font-medium">Username:</span>{" "}
              {profile?.username ?? "Not set username)"} 
            </div>
            <div className="text-sm text-muted-foreground">
              Profile created at: {profile?.created_at ? new Date(profile.created_at).toLocaleString() : "-"}
            </div>
          </>
        )}
      </div>

      {/* Salasanan vaihto */}
    </div>
  );
}
