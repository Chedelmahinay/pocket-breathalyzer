import { EnvVarWarning } from "@/components/env-var-warning";
import { AuthButton } from "@/components/auth-button";
import { Hero } from "@/components/hero/Hero";
import { ThemeSwitcher } from "@/components/theme-switcher";
import { Navbar } from "@/components/ui/navbar";
import { hasEnvVars } from "@/lib/utils";
import Link from "next/link";
import { Suspense } from "react";

export default function Home() {
  return (
    <main className="min-h-screen flex flex-col items-center">
      <div className="flex-1 w-full flex flex-col gap-20 items-center">
        <Navbar
          logo={<Link href="/#home">Smart Al</Link>}
          mobileActionsSide="right"
          links={[
            { href: "/protected", label: "Dashboard" },
            { href: "/#guide", label: "Guide" },
            { href: "/#project", label: "Project" },
          ]}
          actions={
            !hasEnvVars ? (
              <EnvVarWarning />
            ) : (
              <Suspense>
                <AuthButton showGoProtected />
              </Suspense>
            )
          }
        />
        <div className="flex-1 flex flex-col gap-20 max-w-5xl p-5">
          <Hero />
          <main className="flex-1 flex flex-col gap-6 px-4">
          </main>
        </div>
      </div>
      <div className="fixed bottom-4 right-4 z-50 rounded-md border border-foreground/10 bg-background/80 backdrop-blur">
        <ThemeSwitcher />
      </div>
    </main>
  );
}
