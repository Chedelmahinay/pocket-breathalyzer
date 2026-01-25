import { UserCard } from '@/components/UserCard';

export default function ProfilePage() {
  return (
    <div className="mx-auto max-w-2xl p-6 space-y-6">
      <h1 className="text-2xl font-semibold">Profile</h1>
      <UserCard />
    </div>
  );
}
