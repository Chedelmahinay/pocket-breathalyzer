import type { HeroStep } from "./heroContent";

type StepCardProps = {
  step: HeroStep;
};

export function StepCard({ step }: StepCardProps) {
  return (
    <div className="relative overflow-visible rounded-2xl border border-slate-200 bg-slate-50 backdrop-blur min-h-[160px] p-6 pt-10 flex flex-col items-center text-center dark:bg-white/5 dark:border-white/10">
      {/* number badge*/}
      <div className="absolute top-1 left-1 inline-flex items-center justify-center w-8 h-7 rounded-md border border-slate-200 bg-slate-100 text-slate-700 font-semibold dark:bg-white/10 dark:border-white/15 dark:text-white">
        {step.n.replace(".", "")}
      </div>

      <div className="text-base sm:text-lg font-semibold leading-snug">
        {step.text}
        {step.title && (
          <div className="mt-1 text-sm font-normal text-slate-600 dark:text-white/70">
            {step.title}
          </div>
        )}
      </div>
    </div>
  );
}
