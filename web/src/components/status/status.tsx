import { useQuery } from '../../hooks/use-query';
import { getStatus } from '../../lib/queries';
import { Tag } from '../tag/tag';

export function Status() {
  const status = useQuery(getStatus, { refetchInterval: 3000 });

  if (status.error) {
    return <Tag variant="error" label="Offline" />;
  }
  if (status.data?.busy) {
    return <Tag variant="success" label="Busy" />;
  }
  return <Tag variant="normal" label="Idle" />;
}
