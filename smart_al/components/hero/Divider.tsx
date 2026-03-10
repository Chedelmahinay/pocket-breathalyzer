type DividerProps = {
  className?: string;
};

export function Divider({ className = "" }: DividerProps) {
  return (
    <div
      className={[
        "w-full h-px bg-gradient-to-r from-transparent via-slate-300 to-transparent dark:via-white/10",
        className,
      ].join(" ")}
    />
  );
}
