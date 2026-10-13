/* Expose the pointer expected by the CM11 PN547 firmware loader. */
extern const unsigned char gphDnldNfc_DlSequence[];

const unsigned char *gphDnldNfc_DlSeq = gphDnldNfc_DlSequence;
