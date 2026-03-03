import { redirect } from "next/navigation";
import { Suspense, use } from "react";
import { createClient } from "@/lib/supabase/server";
import Link from "next/link";

type ResultRow = {
  rank: number | null;
  username: string | null;
  value: number | null;
};

type ResultsPageProps = {
  searchParams?: Promise<{
    page?: string;
  }>;
};

export default function ResultsPage({ searchParams }: ResultsPageProps) {
  return (
    <div className="mx-auto w-full max-w-6xl p-6 space-y-6">
      <div>
        <h1 className="text-2xl font-semibold text-slate-900 dark:text-slate-100">
          Weekly Ranking
        </h1>
        <p className="text-sm text-slate-500 dark:text-slate-400">
          current week results across all users
        </p>
      </div>

      <Suspense
        fallback={
          <div className="rounded-lg border border-slate-200 p-4 text-sm text-slate-500 dark:border-slate-800 dark:text-slate-400">
            Loading results…
          </div>
        }
      >
        <ResultsTableWrapper searchParams={searchParams} />
      </Suspense>
    </div>
  );
}

function ResultsTableWrapper({
  searchParams,
}: {
  searchParams?: Promise<{ page?: string }>;
}) {
  // Resolve search params inside Suspense to avoid blocking the route.
  const resolvedParams = use(
    searchParams ?? Promise.resolve<{ page?: string }>({})
  );
  const rawPage = resolvedParams.page ?? "1";
  const parsedPage = Number.parseInt(rawPage, 10);
  const currentPage = Number.isFinite(parsedPage) && parsedPage > 0 ? parsedPage : 1;

  return <ResultsTable page={currentPage} />;
}

async function ResultsTable({ page }: { page: number }) {
  const supabase = await createClient();

  const { data: authData, error: authError } = await supabase.auth.getUser();
  if (authError || !authData?.user) redirect("/auth/login");

  // Server-side pagination (10 rows per page) (sivutus)
  const pageSize = 10;
  const begin = (page - 1) * pageSize;
  const end = begin + pageSize - 1; 

  const { data: dbData, error, count } = await supabase
    .from("weekly_leaderboard_current_week") // This view joins measurements with users to get the username in one query.
    .select("rank, username, value", { count: "exact" })
    .order("rank", { ascending: true })
    .range(begin, end);

  if (error) {
    return (
      <div className="rounded-lg border border-red-200 bg-red-50 p-4 text-sm text-red-700">
        Failed to load results: {error.message}
      </div>
    );
  }

  // Defensive sort to keep order stable even if backend changes.
  const data =
    (dbData as ResultRow[] | null)?.sort((a, b) => {
      const rankA = a.rank ?? Number.POSITIVE_INFINITY;
      const rankB = b.rank ?? Number.POSITIVE_INFINITY;
      if (rankA !== rankB) return rankA - rankB;
      const valueA = a.value ?? Number.NEGATIVE_INFINITY;
      const valueB = b.value ?? Number.NEGATIVE_INFINITY;
      return valueB - valueA;
    }) ?? null;

  const total = count ?? 0;
  const totalPages = Math.max(1, Math.ceil(total / pageSize));
  const hasPrev = page > 1;
  const hasNext = page < totalPages;

  return (
    <div className="space-y-4">
      <div className="overflow-hidden rounded-2xl border border-slate-200 dark:border-slate-800">
        <div className="overflow-x-auto">
          <table className="min-w-full text-left text-sm">
            <thead className="bg-slate-50 text-xs uppercase tracking-wide text-slate-500 dark:bg-slate-900/40 dark:text-slate-400">
              <tr>
                <th className="px-4 py-3 font-medium">Rank</th>
                <th className="px-4 py-3 font-medium">Username</th>
                <th className="px-4 py-3 font-medium">Value</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-200 dark:divide-slate-800">
              {data?.length ? (
                data.map((row, index) => (
                  <tr
                    key={`${row.username ?? "user"}-${row.rank ?? index}`}
                    className="text-slate-700 dark:text-slate-200"
                  >
                    <td className="px-4 py-3 font-medium">
                      {row.rank ?? "-"}
                    </td>
                    <td className="px-4 py-3 font-medium">
                      {row.username ?? "Unknown"}
                    </td>
                    <td className="px-4 py-3">{row.value ?? "-"}</td>
                  </tr>
                ))
              ) : (
                <tr>
                  <td
                    className="px-4 py-6 text-center text-slate-500 dark:text-slate-400"
                    colSpan={3}
                  >
                    No results yet.
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>

      <div className="flex items-center justify-between text-sm">
        <div className="text-slate-500 dark:text-slate-400">
          Page {page} of {totalPages}
        </div>
        <div className="flex items-center gap-2">
          {hasPrev ? (
            <Link
              href={`?page=${page - 1}`}
              className="rounded-lg border border-slate-200 px-3 py-1.5 text-slate-700 hover:bg-slate-50 dark:border-slate-800 dark:text-slate-200 dark:hover:bg-slate-900/40"
            >
              Prev
            </Link>
          ) : (
            <span className="rounded-lg border border-slate-200 px-3 py-1.5 text-slate-400 dark:border-slate-800 dark:text-slate-600">
              Prev
            </span>
          )}
          {hasNext ? (
            <Link
              href={`?page=${page + 1}`}
              className="rounded-lg border border-slate-200 px-3 py-1.5 text-slate-700 hover:bg-slate-50 dark:border-slate-800 dark:text-slate-200 dark:hover:bg-slate-900/40"
            >
              Next
            </Link>
          ) : (
            <span className="rounded-lg border border-slate-200 px-3 py-1.5 text-slate-400 dark:border-slate-800 dark:text-slate-600">
              Next
            </span>
          )}
        </div>
      </div>
    </div>
  );
}
