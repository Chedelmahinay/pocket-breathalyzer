import type { HeroStep } from "./heroContent";

type StepCardProps = {
  step: HeroStep;
};

export function StepCard({ step }: StepCardProps) {
  return (
    <div className="relative overflow-visible rounded-2xl bg-white/5 border border-white/10 backdrop-blur min-h-[160px] p-6 pt-10 flex flex-col items-center text-center">
      {/* number badge*/}
      <div className="absolute top-1 left-1 inline-flex items-center justify-center w-8 h-7 rounded-md bg-white/10 border border-white/15 text-white font-semibold">
        {step.n.replace(".", "")}
      </div>

      <div className="text-base sm:text-lg font-semibold leading-snug">
        {step.text}
        {step.title && (
          <div className="mt-1 text-sm font-normal text-white/70">
            {step.title}
          </div>
        )}
      </div>
    </div>
  );
}
