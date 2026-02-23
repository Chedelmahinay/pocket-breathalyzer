import { LoginForm } from "@/components/login-form";
import { createClient } from "@/lib/supabase/server";
import { redirect } from "next/navigation";
import { connection } from "next/server";

export default async function Page() {
  // This page reads request cookies for auth and may redirect per-user.
  // Opt out of prerendering so Next doesn't treat it as a blocking static route.
  await connection();

  const supabase = await createClient();
  const {
    data: { user },
  } = await supabase.auth.getUser();
  // If user is already logged in, redirect to protected page
  if (user) {
    redirect("/protected");
  }

  return (
    <div className="flex min-h-svh w-full items-center justify-center p-6 md:p-10">
      <div className="w-full max-w-sm">
        <LoginForm />
      </div>
    </div>
  );
}
