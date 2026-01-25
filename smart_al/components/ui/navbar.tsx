// components/ui/navbar.tsx
import Link from "next/link";
import { ReactNode } from "react";

interface NavbarProps {
  logo?: ReactNode;
  links?: Array<{ href: string; label: string }>;
  actions?: ReactNode;
}

export function Navbar({ logo, links = [], actions }: NavbarProps) {
  return (
    <nav className="w-full flex justify-center border-b border-b-foreground/10 h-16">
      <div className="w-full max-w-5xl flex justify-between items-center p-3 px-5 text-sm">
        <div className="flex gap-5 items-center font-semibold">
          {logo}
          {links.map((link) => (
            <Link key={link.href} href={link.href}>
              {link.label}
            </Link>
          ))}
        </div>
        {actions}
      </div>
    </nav>
  );
}