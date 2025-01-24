import { ReactNode } from 'preact/compat';
import s from './button.module.css';

interface ButtonGroupProps {
  children: ReactNode;
}

export function ButtonGroup({ children }: ButtonGroupProps) {
  return <div className={s.group}>{children}</div>;
}
