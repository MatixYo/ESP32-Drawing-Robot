import { ButtonHTMLAttributes } from 'preact/compat';
import cn from 'clsx';
import s from './button.module.css';

interface ButtonProps extends ButtonHTMLAttributes<HTMLButtonElement> {
  label: string;
  active?: boolean;
}

export function Button({ label, active, ...props }: ButtonProps) {
  return (
    <button
      {...props}
      type="button"
      className={cn(s.button, active && s.active)}
    >
      {label}
    </button>
  );
}
