'use client';

import { useState } from 'react';
import { createClient } from '@/lib/supabase/client';

interface EditProfileFormProps {
  userId: string;
  initialUsername: string;
  initialName?: string | null;
  initialEmail: string;
  initialAge?: number | null;
  initialWeight?: number | null;
  initialHeight?: number | null;
  onSaved?: (next: {
    username: string;
    name?: string | null;
    email: string;
    age?: number | null;
    weight?: number | null;
    height?: number | null;
  }) => void;
  onCancel?: () => void;
}

export function EditProfileForm({
  userId,
  initialUsername,
  initialName,
  initialEmail,
  initialAge,
  initialWeight,
  initialHeight,
  onSaved,
  onCancel,
}: EditProfileFormProps) {
  const [username, setUsername] = useState(initialUsername);
  const [name, setName] = useState(initialName ?? '');
  const [email, setEmail] = useState(initialEmail);
  const [age, setAge] = useState(initialAge?.toString() ?? '');
  const [weight, setWeight] = useState(initialWeight?.toString() ?? '');
  const [height, setHeight] = useState(initialHeight?.toString() ?? '');
  const [saving, setSaving] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [notice, setNotice] = useState<string | null>(null);
  const supabase = createClient();

  const handleSubmit = async (event: React.FormEvent) => {
    event.preventDefault();
    setSaving(true);
    setError(null);
    setNotice(null);

    const nextUsername = username.trim();
    const nextName = name.trim() || null;
    const nextEmail = email.trim();
    const nextAge = age.trim() === '' ? null : Number(age);
    const nextWeight = weight.trim() === '' ? null : Number(weight);
    const nextHeight = height.trim() === '' ? null : Number(height);

    if (!nextUsername) {
      setError('Username is required.');
      setSaving(false);
      return;
    }

    if (Number.isNaN(nextAge) || Number.isNaN(nextWeight) || Number.isNaN(nextHeight)) {
      setError('Age, weight, and height must be numbers.');
      setSaving(false);
      return;
    }

    try {
      const { error: profileError } = await supabase
        .from('profiles')
        .update({
          username: nextUsername,
          name: nextName,
          age: nextAge,
          weight: nextWeight,
          height: nextHeight,
        })
        .eq('id', userId);

      if (profileError) throw profileError;

      if (nextEmail && nextEmail !== initialEmail) {
        const { error: authError } = await supabase.auth.updateUser({
          email: nextEmail,
        });
        if (authError) throw authError;
        setNotice('Email change requested. Check your inbox to confirm.');
      }

      const { error: emailSyncError } = await supabase
        .from('profiles')
        .update({ email: nextEmail })
        .eq('id', userId);

      if (emailSyncError) throw emailSyncError;

      onSaved?.({
        username: nextUsername,
        name: nextName,
        email: nextEmail,
        age: nextAge,
        weight: nextWeight,
        height: nextHeight,
      });
    } catch (err) {
      const message = err instanceof Error ? err.message : 'Update failed';
      setError(message);
    } finally {
      setSaving(false);
    }
  };

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      <div>
        <label className="block text-sm font-medium text-slate-600 dark:text-slate-300">
          Username
        </label>
        <input
          className="mt-1 w-full rounded-md border border-slate-200 bg-white px-3 py-2 text-slate-900 dark:border-slate-700 dark:bg-black dark:text-slate-100"
          value={username}
          onChange={(event) => setUsername(event.target.value)}
          required
        />
      </div>
      <div>
        <label className="block text-sm font-medium text-slate-600 dark:text-slate-300">
          Name
        </label>
        <input
          className="mt-1 w-full rounded-md border border-slate-200 bg-white px-3 py-2 text-slate-900 dark:border-slate-700 dark:bg-black dark:text-slate-100"
          value={name}
          onChange={(event) => setName(event.target.value)}
        />
      </div>
      <div>
        <label className="block text-sm font-medium text-slate-600 dark:text-slate-300">
          Email
        </label>
        <input
          type="email"
          className="mt-1 w-full rounded-md border border-slate-200 bg-white px-3 py-2 text-slate-900 dark:border-slate-700 dark:bg-black dark:text-slate-100"
          value={email}
          onChange={(event) => setEmail(event.target.value)}
          required
        />
      </div>
      <div>
        <label className="block text-sm font-medium text-slate-600 dark:text-slate-300">
          Age
        </label>
        <input
          type="number"
          className="mt-1 w-full rounded-md border border-slate-200 bg-white px-3 py-2 text-slate-900 dark:border-slate-700 dark:bg-black dark:text-slate-100"
          value={age}
          onChange={(event) => setAge(event.target.value)}
          min={0}
        />
      </div>
      <div>
        <label className="block text-sm font-medium text-slate-600 dark:text-slate-300">
          Weight
        </label>
        <input
          type="number"
          className="mt-1 w-full rounded-md border border-slate-200 bg-white px-3 py-2 text-slate-900 dark:border-slate-700 dark:bg-black dark:text-slate-100"
          value={weight}
          onChange={(event) => setWeight(event.target.value)}
          min={0}
          step="0.1"
        />
      </div>
      <div>
        <label className="block text-sm font-medium text-slate-600 dark:text-slate-300">
          Height
        </label>
        <input
          type="number"
          className="mt-1 w-full rounded-md border border-slate-200 bg-white px-3 py-2 text-slate-900 dark:border-slate-700 dark:bg-black dark:text-slate-100"
          value={height}
          onChange={(event) => setHeight(event.target.value)}
          min={0}
          step="0.1"
        />
      </div>

      {error && <p className="text-sm text-red-600">{error}</p>}
      {notice && <p className="text-sm text-emerald-600">{notice}</p>}

      <div className="flex flex-wrap gap-2">
        <button
          type="submit"
          className="rounded-md bg-slate-900 px-4 py-2 text-sm font-semibold text-white hover:bg-slate-800 disabled:opacity-60 dark:bg-white dark:text-slate-900 dark:hover:bg-slate-100"
          disabled={saving}
        >
          {saving ? 'Saving...' : 'Save changes'}
        </button>
        {onCancel && (
          <button
            type="button"
            className="rounded-md border border-slate-200 px-4 py-2 text-sm font-medium text-slate-600 hover:bg-slate-50 dark:border-slate-700 dark:text-slate-200 dark:hover:bg-slate-900"
            onClick={onCancel}
          >
            Cancel
          </button>
        )}
      </div>
    </form>
  );
}
