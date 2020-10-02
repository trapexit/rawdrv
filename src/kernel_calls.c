void (*kprintf)(char*,…);

void
init_svc_kprintf()
{
  uint32 swi_idx=0xe;
  uint32 ptr;

  swi_idx += KernelBase.kb.f_MaxUserFunctions + 1;
  ptr      = (uint32)KernelBase;
  ptr     -= (swi_idx << 2);
  kprintf  = (void*)ptr;
}
