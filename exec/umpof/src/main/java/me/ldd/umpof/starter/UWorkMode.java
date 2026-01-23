package me.ldd.umpof.starter;

public enum UWorkMode {
    NONSET("none"),

    RT1("rt1"),
    PRT("prt"),
    PRTf("prtf"),

    dkF("dkF")

    ;

    public final String keyStr;

    UWorkMode(String keyStr) {
        this.keyStr = keyStr;
    }

    public static UWorkMode rmod(String str) {
        for (UWorkMode um : UWorkMode.values()) {
            if (um.keyStr.equalsIgnoreCase(str)) {
                return um;
            }
        }
        return null;
    }
}
