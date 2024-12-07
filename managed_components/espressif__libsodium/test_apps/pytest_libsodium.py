import pytest


@pytest.mark.generic
def test_libsodium(dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
