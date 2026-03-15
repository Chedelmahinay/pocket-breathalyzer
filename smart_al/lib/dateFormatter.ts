export function formatFinnishDateTime(timestamp: string): string {
  const date = new Date(timestamp);
  if (Number.isNaN(date.getTime())) return "-";

  const datePart = new Intl.DateTimeFormat("fi-FI", {
    dateStyle: "short",
    timeZone: "Europe/Helsinki",
  }).format(date);
  const timePart = new Intl.DateTimeFormat("fi-FI", {
    hour: "2-digit",
    minute: "2-digit",
    timeZone: "Europe/Helsinki",
  }).format(date);

  return `${datePart} ${timePart}`;
}
