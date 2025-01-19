import { useState, useEffect } from 'preact/hooks';

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
  queryFn: (signal: AbortSignal) => Promise<T>,
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

    const controller = new AbortController(); // Create an AbortController instance
    const { signal } = controller; // Get the signal

    const fetchData = async () => {
      setIsLoading(true);
      try {
        const response = await queryFn(signal); // Pass the signal to the query function
        setData(response);
        setError(null);
      } catch (err) {
        if (err instanceof Error && err.name !== 'AbortError') {
          setError(err instanceof Error ? err : new Error('Unknown error'));
        }
      } finally {
        if (!signal.aborted) {
          setIsLoading(false);
        }
      }
    };

    fetchData();

    if (refetchInterval) {
      const intervalId = setInterval(fetchData, refetchInterval);
      return () => {
        clearInterval(intervalId);
        controller.abort(); // Abort the ongoing request when cleaning up
      };
    }

    return () => {
      controller.abort(); // Abort the ongoing request if the effect is cleaned up
    };
  }, [queryFn, enabled, refetchInterval]);

  return { data, error, isLoading };
}
