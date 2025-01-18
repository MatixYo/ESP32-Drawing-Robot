import s from './card.module.css';

interface CardProps {
  title?: string;
  children: React.ReactNode;
}

export function Card({ title, children }: CardProps) {
  return (
    <div className={s.card}>
      {!!title && <h2 className={s.title}>{title}</h2>}
      {children}
    </div>
  );
}
