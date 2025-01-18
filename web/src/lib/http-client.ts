async function request<T>(url: string, options: RequestInit = {}): Promise<T> {
  const response = await fetch(url, options);

  if (!response.ok) {
    throw new Error(response.statusText);
  }

  return response.json() as T;
}

export function get<T>(url: string): Promise<T> {
  return request(url);
}

export function post<T, D extends Record<string, any>>(
  url: string,
  data?: D,
): Promise<T> {
  const body = new FormData();
  for (const key in data) {
    body.append(key, data[key]);
  }

  return request(url, {
    method: 'POST',
    body: data ? body : undefined,
  });
}
