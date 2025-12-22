type DividerProps = {
  className?: string;
};

export function Divider({ className = "" }: DividerProps) {
  return (
    <div
      className={[
        "w-full h-px bg-gradient-to-r from-transparent via-white/10 to-transparent",
        className,
      ].join(" ")}
    />
  );
}
