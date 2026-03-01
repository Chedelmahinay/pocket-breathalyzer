import Link from "next/link";
import { Button } from "./ui/button";
import { createClient } from "@/lib/supabase/server";
import { LogoutButton } from "./logout-button";

type AuthButtonProps = {
  showGoProtected?: boolean;
};

export async function AuthButton({ showGoProtected = false }: AuthButtonProps) {
  const supabase = await createClient();

  const {
    data: { user },
  } = await supabase.auth.getUser();

   if (!user) {
    return (
      <div className="flex gap-2">
        <Button asChild size="lg" variant={"outline"} className="px-8 py-6 text-lg">
          <Link href="/auth/login">Sign in</Link>
        </Button> 
        <Button asChild size="lg" variant={"default"} className="px-8 py-6 text-lg">
          <Link href="/auth/sign-up">Sign up</Link>
        </Button>
      </div>
    );
  }

  const { data: profile } = await supabase
    .from("profiles")
    .select("username")
    .eq("id", user.id)
    .maybeSingle();

  const username = profile?.username ?? user.email;

  return (
    <div className="flex items-center gap-4">
      {/* Only on Landing Page when enabled */}
      {showGoProtected && (
        <Link
          href="/protected"
          className="text-sm font-semibold text-blue-400 hover:text-blue-300 transition-colors">   
          Open MyApp Here
        </Link>
      )}

      <Link
        href="/protected/account"
        className="text-sm font-medium text-slate-700 hover:text-slate-900 dark:text-slate-200 dark:hover:text-white"
      >
        {username}
      </Link>

      <LogoutButton />
    </div>
  );
}


  
