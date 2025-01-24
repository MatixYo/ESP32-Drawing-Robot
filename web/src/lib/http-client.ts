async function request<T>(url: string, options: RequestInit = {}): Promise<T> {
  const response = await fetch(url, options);

  if (!response.ok) {
    throw new Error(response.statusText);
  }

  const contentType = response.headers.get('content-type');
  if (contentType?.includes('application/json')) {
    return response.json() as T;
  }
  return response.text() as T;
}

export function get<T>(url: string, options?: RequestInit): Promise<T> {
  return request(url, options);
}

export function post<T, D extends Record<string, unknown>>(
  url: string,
  data?: D,
  options?: RequestInit,
): Promise<T> {
  const body = new FormData();
  for (const key in data) {
    body.append(key, data[key] as string | Blob);
  }

  return request(url, {
    method: 'POST',
    body: data ? body : undefined,
    ...options,
  });
}
