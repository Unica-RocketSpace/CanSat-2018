import sys
import logging

from argparse import ArgumentParser

from pymavlink import mavutil
from pymavlink.dialects.v20.UNISAT import *

from . import _log as _root_log

_log = _root_log.getChild("main")


def process_message(msg):
        # _log.info("%s", msg)
    if isinstance(msg, MAVLink_atmega_message):
        _log.info(
            "{cseq: %d, time : %0.3f, pressure : %0.3f, height : %0.2f, temp : %0.1f, state : %d}"
            %
            (msg.get_header().seq, msg.time, msg.pressure, msg.height, msg.temp, msg.state)
        )

    _log.info(msg)



def main(argv):
    parser = ArgumentParser(add_help=True)
    parser.add_argument("mavuri", nargs="?", action="store", default="udpin:0.0.0.0:12000")

    logging.basicConfig(
        stream=sys.stdout, level=logging.INFO,
        format="%(asctime)-15s %(message)s"
    )

    args = parser.parse_args(argv)
    mavuri = args.mavuri

    _log.info("Запускаюсь. Использую url: %s" % mavuri)
    connection1 = mavutil.mavlink_connection("udpin:0.0.0.0:12000")
    connection2 = mavutil.mavlink_connection("udpin:0.0.0.0:12001")
    mav1, mav2 = connection1, connection2

    while True:
        msg1 = mav1.recv_match(blocking=False)
        msg2 = mav2.recv_match(blocking=False)

        if msg1:
            process_message(msg1)

        if msg2:
            process_message(msg2)


if __name__ == "__main__":
    exit(main(sys.argv[1:]))
