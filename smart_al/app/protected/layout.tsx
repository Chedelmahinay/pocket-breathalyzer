import { EnvVarWarning } from "@/components/env-var-warning";
import { AuthButton } from "@/components/auth-button";
import { ThemeSwitcher } from "@/components/theme-switcher";
import { Navbar } from "@/components/ui/navbar";
import { hasEnvVars } from "@/lib/utils";
import Link from "next/link";
import { Suspense } from "react";

export default function ProtectedLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <main className="min-h-screen flex flex-col items-center">
      <div className="flex-1 w-full flex flex-col gap-20 items-center">
        <Navbar
          logo={<Link href={"/"}>Smart-Al Meter</Link>}
          mobileInlineActionsSide="right"
          links={[
            { href: "/protected/history", label: "History" },
            { href: "/protected/ranking", label: "Ranking" },
            { href: "/protected/chat", label: "Chat" },
            { href: "/protected/account", label: "Profile" },
          ]}
          actions={
            !hasEnvVars ? (
              <EnvVarWarning />
            ) : (
              <Suspense>
                <AuthButton />
              </Suspense>
            )
          }
        />
        <div className="flex-1 flex flex-col gap-20 max-w-5xl p-5">
          {children}
        </div>

        <footer className="w-full flex items-center justify-center border-t mx-auto text-center text-xs gap-8 py-16">
          <ThemeSwitcher />
        </footer>
      </div>
    </main>
  );
}
