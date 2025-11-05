/* Stub implementations for FD (Finite Domain) functions when building with -DNOFD */

#ifdef NOFD

int fd_new_label() {
    return 0;
}

void fd_add_instr(i)
    int i;
{
    /* No-op */
}

void fd_add_const(i)
    int i;
{
    /* No-op */
}

void fd_pop_check_label() {
    /* No-op */
}

void fd_push_check_label() {
    /* No-op */
}

void initialize_finite() {
    /* No-op */
}

#endif
