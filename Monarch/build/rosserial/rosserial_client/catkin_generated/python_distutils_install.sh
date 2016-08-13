#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
    DESTDIR_ARG="--root=$DESTDIR"
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/oduasv/Monarch/src/rosserial/rosserial_client"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/oduasv/Monarch/install/lib/python2.7/dist-packages:/home/oduasv/Monarch/build/lib/python2.7/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/oduasv/Monarch/build" \
    "/usr/bin/python" \
    "/home/oduasv/Monarch/src/rosserial/rosserial_client/setup.py" \
    build --build-base "/home/oduasv/Monarch/build/rosserial/rosserial_client" \
    install \
    $DESTDIR_ARG \
    --install-layout=deb --prefix="/home/oduasv/Monarch/install" --install-scripts="/home/oduasv/Monarch/install/bin"
