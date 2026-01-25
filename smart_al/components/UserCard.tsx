'use client';

import { useEffect, useState, useMemo } from 'react';
import { createClient } from "@/lib/supabase/client";

interface User {
  username: string;
  email: string;
  created_at: string;
}

export function UserCard() {
  const [user, setUser] = useState<User | null>(null);
  const [loading, setLoading] = useState(true);
  const supabase = useMemo(() => createClient(), []);

  useEffect(() => {
    const fetchUser = async () => {
      try {
        const { data: { user: authUser } } = await supabase.auth.getUser();
        if (authUser) {
          const { data, error } = await supabase
            .from('profiles')
            .select('username, email, created_at')
            .eq('id', authUser.id)
            .single();

          if (error) throw error;
          setUser(data);
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
    <div className="bg-white rounded-lg shadow p-6 space-y-4">
      <div>
        <label className="block text-sm font-medium text-gray-700">Username</label>
        <p className="mt-1 text-lg text-gray-900">{user.username}</p>
      </div>
      <div>
        <label className="block text-sm font-medium text-gray-700">Email</label>
        <p className="mt-1 text-lg text-gray-900">{user.email}</p>
      </div>
      <div>
        <label className="block text-sm font-medium text-gray-700">Account Created</label>
        <p className="mt-1 text-lg text-gray-900">
          {new Date(user.created_at).toLocaleDateString()}
        </p>
      </div>
    </div>
  );
}