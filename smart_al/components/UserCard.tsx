'use client';

import { useEffect, useState, useMemo } from 'react';
import { createClient } from "@/lib/supabase/client";
import { EditProfileForm } from '@/components/edit-profile-form';

interface User {
  username: string;
  name?: string | null;
  email: string;
  age?: number | null;
  weight?: number | null;
  height?: number | null;
  created_at: string;
}

export function UserCard() {
  const [user, setUser] = useState<User | null>(null);
  const [loading, setLoading] = useState(true);
  const [isEditing, setIsEditing] = useState(false);
  const [authUserId, setAuthUserId] = useState<string | null>(null);
  const supabase = useMemo(() => createClient(), []);

  useEffect(() => {
    const fetchUser = async () => {
      try {
        const { data: { user: authUser } } = await supabase.auth.getUser();
        if (authUser) {
          const { data, error } = await supabase
            .from('profiles')
            .select('username, email, name, age, weight, height, created_at')
            .eq('id', authUser.id)
            .single();

          if (error) throw error;
          setUser(data);
          setAuthUserId(authUser.id);
        }
      } catch (error) {
        console.error('Failed to fetch user:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchUser();
  }, [supabase]);

  if (loading) return <div className="text-center">Loading...</div>;
  if (!user) return <div className="text-center text-red-500">Failed to load user</div>;

  return (
    <div className="space-y-5">
      <div className="flex items-center justify-between border-b border-slate-200 pb-4 dark:border-slate-800">
        <div>
          <p className="text-sm font-medium text-slate-500 dark:text-slate-400">Profile</p>
          <p className="mt-1 text-xl font-semibold text-slate-900 dark:text-slate-100">
            {isEditing ? 'Editing' : user.username}
          </p>
        </div>
        <button
          type="button"
          onClick={() => setIsEditing((prev) => !prev)}
          className="text-sm font-medium text-slate-600 hover:text-slate-900 dark:text-slate-300 dark:hover:text-white"
        >
          {isEditing ? 'Close' : 'Edit'}
        </button>
      </div>

      {isEditing && authUserId ? (
        <EditProfileForm
          userId={authUserId}
          initialUsername={user.username}
          initialName={user.name}
          initialEmail={user.email}
          initialAge={user.age}
          initialWeight={user.weight}
          initialHeight={user.height}
          onSaved={(next) => {
            setUser((prev) => (prev ? { ...prev, ...next } : prev));
            setIsEditing(false);
          }}
          onCancel={() => setIsEditing(false)}
        />
      ) : (
        <div className="space-y-3 text-sm">
          <div className="grid grid-cols-[140px_1fr] items-center gap-6 border-b border-slate-200 pb-3 dark:border-slate-800">
            <span className="text-slate-500 dark:text-slate-400">Name</span>
            <span className="font-medium text-slate-900 dark:text-slate-100">{user.name || '-'}</span>
          </div>
          <div className="grid grid-cols-[140px_1fr] items-center gap-6 border-b border-slate-200 pb-3 dark:border-slate-800">
            <span className="text-slate-500 dark:text-slate-400">Age</span>
            <span className="font-medium text-slate-900 dark:text-slate-100">
              {user.age ?? '-'}
            </span>
          </div>
          <div className="grid grid-cols-[140px_1fr] items-center gap-6 border-b border-slate-200 pb-3 dark:border-slate-800">
            <span className="text-slate-500 dark:text-slate-400">Weight</span>
            <span className="font-medium text-slate-900 dark:text-slate-100">
              {user.weight ?? '-'}
            </span>
          </div>
          <div className="grid grid-cols-[140px_1fr] items-center gap-6 border-b border-slate-200 pb-3 dark:border-slate-800">
            <span className="text-slate-500 dark:text-slate-400">Height</span>
            <span className="font-medium text-slate-900 dark:text-slate-100">
              {user.height ?? '-'}
            </span>
          </div>
          <div className="grid grid-cols-[140px_1fr] items-center gap-6 border-b border-slate-200 pb-3 dark:border-slate-800">
            <span className="text-slate-500 dark:text-slate-400">Email</span>
            <span className="font-medium text-slate-900 dark:text-slate-100 break-all">{user.email}</span>
          </div>
          <div className="grid grid-cols-[140px_1fr] items-center gap-6">
            <span className="text-slate-500 dark:text-slate-400">Account Created</span>
            <span className="font-medium text-slate-900 dark:text-slate-100">
              {new Date(user.created_at).toLocaleDateString()}
            </span>
          </div>
        </div>
      )}
    </div>
  );
}
