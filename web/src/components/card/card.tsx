import { useState } from 'preact/hooks';
import { Button } from '../button/button';
import s from './card.module.css';

interface CardProps {
  title?: React.ReactNode;
  children: React.ReactNode;
  expandable?: boolean;
}

export function Card({ title, children, expandable }: CardProps) {
  const [expanded, setExpanded] = useState(false);
  const toggle = () => setExpanded(!expanded);

  return (
    <div className={s.card}>
      {!!title && (
        <h2 className={s.title}>
          {title}
          {expandable && (
            <Button label={expanded ? 'Hide' : 'Show'} onClick={toggle} />
          )}
        </h2>
      )}
      {(!expandable || expanded) && children}
    </div>
  );
}
