// basic.d.ts
export interface AsyncCallback<T> {
  (err: BusinessError, data: T): void;
}

export interface BusinessError extends Error {
  code: number;
}

export interface ErrorCallback {
  (err: BusinessError): void;
}

export interface Callback<T> {
  (data: T): void;
}
