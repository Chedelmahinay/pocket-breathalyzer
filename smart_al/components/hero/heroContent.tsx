export type HeroStep = {
  n: string;
  text: string;
  title?: string;
};

export const HERO_STEPS: HeroStep[] = [
  { n: "1", text: "Sign in or Sign up", title: "Select at the top-right in page" },
  { n: "2", text: "Pair your device", title: "Pair the web app to the breathalyzer" },
  { n: "3", text: "Take the test", title: "Take a breath test using device" },
  { n: "4", text: "View results", title: "See alcohol level data in the web or device" },
];

export const KEY_FEATURES: string[] = [
  "User Authentication: Secure sign-up and sign-in functionality.",
  "Device Pairing: Easy pairing of the breathalyzer device via BLE.",
  "Breath Test Functionality: reliable alcohol level measurement.",
  "Data Storage: Results are stored in a database linked to user accounts.",
  "Responsive Web App: Data is accessible on computer and mobile devices for convenience.",
];
