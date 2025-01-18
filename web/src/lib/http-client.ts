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

export function post<T, D>(url: string, data?: D): Promise<T> {
  return request(url, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    ...(data ? { body: JSON.stringify(data) } : {}),
  });
}
