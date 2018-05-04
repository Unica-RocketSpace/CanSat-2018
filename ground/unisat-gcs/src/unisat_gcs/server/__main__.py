import sys
import logging

from argparse import ArgumentParser

from pymavlink import mavutil
from pymavlink.dialects.v20.UNISAT import *

from . import _log as _root_log

_log = _root_log.getChild("main")


def main(argv):
    parser = ArgumentParser(add_help=True)
    parser.add_argument("mavuri", nargs="?", action="store", default="udpin:0.0.0.0:12000")


    logging.basicConfig(stream=sys.stdout, level=logging.INFO)

    args = parser.parse_args(argv)
    mavuri = args.mavuri

    _log.info("Запускаюсь. Использую url: %s" % mavuri)
    connection = mavutil.mavlink_connection(mavuri)
    mav = connection

    while True:
        msg = mav.recv_match(blocking=True)
        _log.info("got message %s", msg)


if __name__ == "__main__":
    exit(main(sys.argv[1:]))
