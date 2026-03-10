import Link from "next/link";
import { Button } from "./ui/button";

export function BlockButton() {
  return (
    <>
      <Link
        href="https://www.youtube.com/watch?v=bZe5J8SVCYQ"
        target="_blank"
      >
        <Button variant="default" className="flex items-center gap-2" size="lg">
          <svg
            className="!h-6 !w-6"
            height="48"
            width="48"
            fill="#1f1f1f"
            viewBox="0 -960 960 960"
            xmlns="http://www.w3.org/2000/svg"
          >
            <path d="M446.67-89.33V-400L256-209.33 209.33-256l224-224-224-224L256-750.67 446.67-560v-310.67H480L698.67-652l-172 172 172 172L480-89.33h-33.33ZM513.33-560l92-92-92-90v182Zm0 342 92-90-92-92v182Z"/>
          </svg>
          <span className="text-base">Connect</span>
        </Button>
      </Link>
    </>
  );
}
