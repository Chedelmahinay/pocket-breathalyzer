import { LoginForm } from "@/components/login-form";
import { createClient } from "@/lib/supabase/server";
import { redirect } from "next/navigation";
import { Suspense } from "react";

export default function Page() {
  return (
    <div className="flex min-h-svh w-full items-center justify-center p-6 md:p-10">
      <div className="w-full max-w-sm">
        <Suspense fallback={<LoginForm />}>
          <LoginGate />
        </Suspense>
      </div>
    </div>
  );
}

async function LoginGate() {
  const supabase = await createClient();
  const {
    data: { user },
  } = await supabase.auth.getUser();

  if (user) {
    redirect("/protected");
  }

  return <LoginForm />;
}
