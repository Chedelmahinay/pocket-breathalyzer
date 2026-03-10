import { Divider } from "./Divider";
import { StepsSection } from "./StepsSection";
import { ProjectSection } from "./ProjectSection";
import { HERO_STEPS, KEY_FEATURES } from "./heroContent";

export function Hero() {
  return (
    <section
      id="home"
      className="flex flex-col items-center text-center gap-8 pt-2 pb-16 sm:pt-0 sm:pb-20"
    >
      {/* Title */}
      <h1 className="text-5xl sm:text-8xl font-bold tracking-wide">
      Smart Al
      </h1>

      {/* Subtitle */}
      <p className="text-lg sm:text-xl text-slate-600 dark:text-gray-300 max-w-xl">
        A student-led IoT breathalyzer project at Metropolia University
      </p>

      <Divider className="max-w-2xl mt-10" />
      {/*Calls Steps in heroContent*/}
      <div id="guide" className="w-full scroll-mt-24">
        <StepsSection title="Easy user guide" steps={HERO_STEPS} />
      </div>

      <Divider className="max-w-2xl mt-10" />

      <div id="project" className="w-full scroll-mt-24">
        <ProjectSection
          description={
            "Smart Al-Test is an IoT breathalyzer system that connects a BLE-enabled device to a web application. Users can securely sign in, pair the device, perform a breath test, and view stored results under their own account."
          }
          howItWorks={
            "The breathalyzer device measures a breath sample and sends the reading to the web app over BLE. The web app displays the result and stores it in the database so the user can review their history later."
          }
          features={KEY_FEATURES}
        />
      </div>
      <div className="w-full max-w-5xl h-px bg-gradient-to-r from-transparent via-slate-300 to-transparent dark:via-white/10 mt-12" />
    </section>
  );
}
