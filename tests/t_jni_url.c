// The web pages an Android intent may open (kl_jni_web_url, KL_JNI_OPEN_URL):
// https only, http upgraded, and only on the listed hosts or under them. Pure
// string checks; no guest, no network. In `make check`.
#include <stdio.h>
#include <string.h>
#include "jni/kl_jni_int.h"

static int fails;
static void expect(const char *in, const char *want) {
    char out[256];
    const char *why = NULL;
    int ok = kl_jni_web_url(in, out, sizeof out, &why);
    int good = want ? ok && !strcmp(out, want) : !ok && why;
    if (!good) fails++;
    printf("  %s  %-48s -> %s\n", good ? "ok  " : "FAIL", in ? in : "(null)",
           ok ? out : why ? why : "(refused, no reason)");
}

int main(void) {
    // Allowed, as given or upgraded.
    expect("https://open.spotify.com/",            "https://open.spotify.com/");
    expect("http://open.spotify.com/track/1",      "https://open.spotify.com/track/1");
    expect("https://accounts.spotify.com/login",   "https://accounts.spotify.com/login");
    expect("https://www.youtube.com/watch?v=x",    "https://www.youtube.com/watch?v=x");
    expect("https://youtu.be/abc",                 "https://youtu.be/abc");
    expect("https://accounts.google.com/",         "https://accounts.google.com/");
    expect("https://www.google.com/",              "https://www.google.com/");
    expect("HTTPS://WWW.Meta.com/quest",           "https://WWW.Meta.com/quest");
    expect("https://www.oculus.com:443/x",         "https://www.oculus.com:443/x");
    expect("https://m.facebook.com",               "https://m.facebook.com");
    expect("https://youtube.com./",                "https://youtube.com./");
    // Refused: other schemes, other hosts, look-alikes, tricks.
    expect("ovrweb://pwa?packageName=x",           NULL);
    expect("javascript:alert(1)",                  NULL);
    expect("file:///etc/passwd",                   NULL);
    expect("https://example.com/",                 NULL);
    expect("https://notyoutube.com/",              NULL);
    expect("https://youtube.com.example.com/",     NULL);
    expect("https://spotify.com@example.com/",     NULL);
    expect("https://user:pw@open.spotify.com/",    NULL);
    expect("https://evil.com\\@youtube.com/",      NULL);
    expect("https://you%74ube.com/",               NULL);
    expect("https:///path",                        NULL);
    expect("https://",                             NULL);
    expect("",                                     NULL);
    expect(NULL,                                   NULL);
    printf(fails ? "FAIL: %d web-address case(s)\n"
                 : "PASS: web addresses for intents: https only, listed hosts only.\n", fails);
    return fails != 0;
}
