import { useState, useEffect } from 'react';

export interface UseQueryOptions<T> {
  enabled?: boolean;
  refetchInterval?: number | null;
  initialData?: T | null;
}

export interface UseQueryResult<T> {
  data: T | null;
  error: Error | null;
  isLoading: boolean;
}

export function useQuery<T>(
  queryFn: () => Promise<T>,
  options: UseQueryOptions<T> = {},
): UseQueryResult<T> {
  const {
    enabled = true,
    refetchInterval = null,
    initialData = null,
  } = options;
  const [data, setData] = useState<T | null>(initialData);
  const [error, setError] = useState<Error | null>(null);
  const [isLoading, setIsLoading] = useState<boolean>(true);

  useEffect(() => {
    if (!enabled) return;

    const fetchData = async () => {
      setIsLoading(true);
      try {
        const response = await queryFn();
        setData(response);
      } catch (err) {
        setError(err instanceof Error ? err : new Error('Unknown error'));
      } finally {
        setIsLoading(false);
      }
    };

    fetchData();

    if (refetchInterval) {
      const intervalId = setInterval(fetchData, refetchInterval);
      return () => clearInterval(intervalId);
    }

    return undefined;
  }, [queryFn, enabled, refetchInterval]);

  return { data, error, isLoading };
}
