type ProjectSectionProps = {
  description: string;
  howItWorks: string;
  features: string[];
};

export function ProjectSection({ description, howItWorks, features }: ProjectSectionProps) {
  return (
    <div className="w-full max-w-5xl mt-20 px-4 sm:px-0">
      <div className="max-w-3xl mx-auto">
        <h2 className="text-3xl sm:text-4xl font-bold tracking-tight text-center">
          Project Description
        </h2>

        <p className="mt-6 text-base text-white/70 leading-relaxed text-center">
          {description}
        </p>

        <h3 className="mt-10 text-2xl sm:text-3xl font-semibold tracking-tight text-center">
          How does it work?
        </h3>
        <p className="mt-4 text-sm sm:text-base text-white/70 leading-relaxed text-center">
          {howItWorks}
        </p>

        <h2 className="mt-10 text-2xl sm:text-3xl font-semibold tracking-tight text-center">
          Key Features
        </h2>

        <div className="mt-4 flex justify-center">
          <ul className="mt-4 space-y-2 text-sm sm:text-base text-white/70 text-center max-w-3xl">
            {features.map((f) => (
              <li key={f} className="flex justify-center gap-2">
                <span>•</span>
                <span>{f}</span>
              </li>
            ))}
          </ul>
        </div>
      </div>
    </div>
  );
}
