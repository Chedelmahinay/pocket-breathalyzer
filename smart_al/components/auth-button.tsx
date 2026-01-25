import Link from "next/link";
import { Button } from "./ui/button";
import { createClient } from "@/lib/supabase/server";
import { LogoutButton } from "./logout-button";

export async function AuthButton() {
  const supabase = await createClient();

  // You can also use getUser() which will be slower.
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
      {username}
      <LogoutButton />
    </div>
  );
}


  

