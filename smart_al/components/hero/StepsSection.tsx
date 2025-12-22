import type { HeroStep } from "./heroContent";
import { StepCard } from "./StepCard";

type StepsSectionProps = {
  title: string;
  steps: HeroStep[];
};

export function StepsSection({ title, steps }: StepsSectionProps) {
  return (
    <>
      {/* Step Title */}
      <div className="w-full max-w-5xl mt-20 px-4 sm:px-0 text-center">
        <h2 className="text-2xl sm:text-5xl font-semibold tracking-tight">
          {title}
        </h2>
      </div>

      {/* Steps */}
      <div className="w-full max-w-5xl mt-0 grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-6 px-2 sm:px-0">
        {steps.map((s) => (
          <StepCard key={s.n} step={s} />
        ))}
      </div>
    </>
  );
}
