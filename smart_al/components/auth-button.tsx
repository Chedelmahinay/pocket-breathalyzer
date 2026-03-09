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
      <div className="flex items-center gap-1.5 md:gap-2">
        <Button
          asChild
          size="sm"
          variant={"outline"}
          className="h-9 px-3 text-sm whitespace-nowrap md:h-12 md:px-8 md:text-lg"
        >
          <Link href="/auth/login">Sign in</Link>
        </Button>
        <Button
          asChild
          size="sm"
          variant={"default"}
          className="h-9 px-3 text-sm whitespace-nowrap md:h-12 md:px-8 md:text-lg"
        >
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
    <div className="flex items-center gap-3 md:gap-4">
      <div className="order-1 md:order-2">
        <LogoutButton />
      </div>
      <Link
        href="/protected/account"
        className="order-2 max-w-24 truncate text-sm font-medium text-slate-700 hover:text-slate-900 dark:text-slate-200 dark:hover:text-white md:order-1 md:max-w-40"
        title={username ?? undefined}
      >
        {username}
      </Link>
    </div>
  );
}


  
