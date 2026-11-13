# PlatformIO extra script: make Digispark uploads work on Windows.
#
# PlatformIO's bundled micronucleus.exe needs libusb0.dll, which Windows
# doesn't have by default ("libusb0.dll was not found").
# 1. If tools/win/ has micronucleus.exe + libusb0.dll (built by
#    tools/build-micronucleus-win.sh or downloaded from the CI artifact),
#    use them as the uploader.
# 2. Otherwise, if libusb0.dll can't be found anywhere, stop the upload
#    with a clear message instead of the Windows popup.
import os
import sys

Import("env")  # noqa: F821  (provided by PlatformIO)

if sys.platform.startswith("win") and env.subst("$UPLOAD_PROTOCOL") == "micronucleus":
    project_dir = env.subst("$PROJECT_DIR")
    win_dir = os.path.join(project_dir, "tools", "win")
    local_exe = os.path.join(win_dir, "micronucleus.exe")
    local_dll = os.path.join(win_dir, "libusb0.dll")

    if os.path.isfile(local_exe) and os.path.isfile(local_dll):
        print("lazyboard: using uploader from tools/win/")
        env.Replace(UPLOADER='"%s"' % local_exe)
    else:
        tool_dir = env.PioPlatform().get_package_dir("tool-micronucleus") or ""
        system_dir = os.path.join(os.environ.get("SystemRoot", r"C:\Windows"), "System32")
        found = any(
            os.path.isfile(os.path.join(d, "libusb0.dll"))
            for d in (tool_dir, system_dir) if d
        )
        if not found:
            def _missing_libusb(*_args, **_kwargs):
                sys.stderr.write(
                    "\nlazyboard: libusb0.dll not found, micronucleus can't run.\n"
                    "Fix (see readme.org -> 'uploading on windows'):\n"
                    "  - put micronucleus.exe + libusb0.dll into tools/win/\n"
                    "    (CI artifact 'micronucleus-win64' or tools/build-micronucleus-win.sh), or\n"
                    "  - install the libusb-win32 driver with Zadig: https://zadig.akeo.ie\n\n"
                )
                env.Exit(1)

            env.AddPreAction("upload", _missing_libusb)
