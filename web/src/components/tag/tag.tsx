import cn from 'clsx';
import s from './tag.module.css';

export interface TagProps {
  variant?: 'normal' | 'success' | 'error';
  label: string;
}

export function Tag({ variant = 'normal', label }: TagProps) {
  return <div className={cn(s.tag, s[variant])}>{label}</div>;
}
