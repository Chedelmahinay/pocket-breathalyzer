"use client";

import Link from "next/link";
import { Menu, X } from "lucide-react";
import { usePathname } from "next/navigation";
import { ReactNode, useEffect, useState } from "react";

import { Button } from "@/components/ui/button";
import { cn } from "@/lib/utils";

interface NavbarProps {
  logo?: ReactNode;
  links?: Array<{ href: string; label: string }>;
  actions?: ReactNode;
  forceHamburger?: boolean;
  forceHamburgerActionsSide?: "left" | "right";
  mobileInlineActionsSide?: "left" | "right";
}

export function Navbar({
  logo,
  links = [],
  actions,
  forceHamburger = false,
  forceHamburgerActionsSide = "left",
  mobileInlineActionsSide,
}: NavbarProps) {
  const [isOpen, setIsOpen] = useState(false);
  const pathname = usePathname();
  const hasLinks = links.length > 0;
  const hasMobileInlineActions = !forceHamburger && !!actions && !!mobileInlineActionsSide;
  const showInlineRightActions =
    !!actions &&
    ((forceHamburger && forceHamburgerActionsSide === "right") ||
      (hasMobileInlineActions && mobileInlineActionsSide === "right"));
  const showInlineLeftActions =
    !!actions &&
    ((forceHamburger && forceHamburgerActionsSide === "left") ||
      (hasMobileInlineActions && mobileInlineActionsSide === "left"));

  useEffect(() => {
    setIsOpen(false);
  }, [pathname]);

  return (
    <nav className="w-full flex justify-center border-b border-b-foreground/10">
      <div
        className={cn(
          "w-full max-w-5xl p-3 px-5 text-sm",
          forceHamburger
            ? "flex flex-col gap-2"
            : "md:h-16 md:flex md:justify-between md:items-center",
        )}
      >
        <div className="flex items-center justify-between gap-3">
          <div className="flex gap-5 items-center font-semibold min-w-0">
            <div className="min-w-0 truncate">{logo}</div>
            {showInlineLeftActions ? (
              <div className={cn("min-w-0 flex items-center", !forceHamburger && "md:hidden")}>
                {actions}
              </div>
            ) : null}
            <div
              className={cn(
                "gap-5 items-center",
                forceHamburger ? "hidden" : "hidden md:flex",
              )}
            >
              {links.map((link) => (
                <Link key={link.href} href={link.href}>
                  {link.label}
                </Link>
              ))}
            </div>
          </div>

          <div className={cn("flex items-center gap-2", forceHamburger && "md:gap-3")}>
            {showInlineRightActions ? (
              <div
                className={cn(
                  "min-w-0 flex items-center justify-end",
                  !forceHamburger && "md:hidden",
                )}
              >
                {actions}
              </div>
            ) : null}

            {hasLinks ? (
              <Button
                type="button"
                variant="ghost"
                size="icon"
                className={cn(
                  !forceHamburger && "md:hidden",
                  forceHamburger && "h-11 w-11 md:h-12 md:w-12",
                )}
                aria-label={isOpen ? "Close navigation menu" : "Open navigation menu"}
                aria-expanded={isOpen}
                aria-controls="mobile-nav-menu"
                onClick={() => setIsOpen((open) => !open)}
              >
                {isOpen ? (
                  <X className={cn(forceHamburger && "h-5 w-5 md:h-6 md:w-6")} />
                ) : (
                  <Menu className={cn(forceHamburger && "h-5 w-5 md:h-6 md:w-6")} />
                )}
              </Button>
            ) : null}
          </div>
        </div>

        {hasLinks ? (
          <div
            className={cn(
              "grid transition-all duration-200 ease-out",
              !forceHamburger && "md:hidden",
              isOpen
                ? "visible grid-rows-[1fr] opacity-100 mt-2"
                : "invisible grid-rows-[0fr] opacity-0 pointer-events-none",
            )}
            aria-hidden={!isOpen}
          >
            <div className="overflow-hidden">
              <div
                id="mobile-nav-menu"
                className={cn(
                  "rounded-md border border-foreground/10 bg-background p-1",
                  forceHamburger &&
                    "w-full sm:ml-auto sm:w-auto sm:min-w-[18rem] md:min-w-[22rem] lg:min-w-[26rem] md:p-2",
                )}
              >
                <div className="flex flex-col">
                  {links.map((link) => (
                    <Link
                      key={link.href}
                      href={link.href}
                      className={cn(
                        "rounded-md px-3 py-2 hover:bg-accent",
                        forceHamburger && "md:px-4 md:py-3 md:text-base",
                      )}
                      onClick={() => setIsOpen(false)}
                    >
                      {link.label}
                    </Link>
                  ))}
                </div>
              </div>
            </div>
          </div>
        ) : null}

        {actions && !forceHamburger ? (
          <div
            className={cn(
              mobileInlineActionsSide ? "hidden md:block" : "mt-2",
              "md:mt-0",
            )}
          >
            {actions}
          </div>
        ) : null}
      </div>
    </nav>
  );
}
