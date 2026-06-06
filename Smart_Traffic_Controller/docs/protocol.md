# Serial Protocol

- **Transport:** USART2, **115200 baud, 8 data bits, no parity, 1 stop bit**.
- **Line termination:** a command is processed on the first `CR` (`\r`) or `LF`
  (`\n`). Empty lines (bare CR/LF) are ignored.
- **Case:** keywords and the `R`/`G`/`Y` prefixes are case-insensitive.
- **Whitespace:** leading/trailing spaces and tabs are trimmed before parsing.
- **Line length:** lines longer than 16 characters are rejected with `NACK`
  (and discarded) — they cannot overflow any buffer.

## Commands

| Input     | Meaning                              | Reply         |
|-----------|--------------------------------------|---------------|
| `R<sec>`  | Set RED phase duration (seconds)     | `ACK` + STATUS |
| `G<sec>`  | Set GREEN phase duration (seconds)   | `ACK` + STATUS |
| `Y<sec>`  | Set YELLOW phase duration (seconds)  | `ACK` + STATUS |
| `START`   | Begin cycling (from RED)             | `ACK` + STATUS |
| `STOP`    | Halt; all signals off, state IDLE    | `ACK` + STATUS |
| `STATUS`  | Report timings and active state      | `ACK` + STATUS |
| `HELP`    | List supported commands              | `ACK` + help   |
| *anything else* | unrecognised / malformed       | `NACK`         |

`<sec>` is a decimal integer in the inclusive range **1 … 3600**. Values
outside the range, with a sign, with non-digits, or that would overflow are
rejected with `NACK` and leave the configuration unchanged.

### Responses

- **`ACK\r\n`** — command accepted.
- **`NACK\r\n`** — command rejected; no state change.

Every *state-changing* command (`R`/`G`/`Y`/`START`/`STOP`) and `STATUS` echoes
a status line after the `ACK`.

### STATUS line format

```
STATUS STATE=<state> RUN=<0|1> R=<sec> G=<sec> Y=<sec> REMAIN=<sec>\r\n
```

| Field    | Meaning                                                       |
|----------|---------------------------------------------------------------|
| `STATE`  | `IDLE`, `RED`, `GREEN`, or `YELLOW`                           |
| `RUN`    | `1` while cycling, `0` when idle/stopped                      |
| `R/G/Y`  | currently configured phase durations (seconds)               |
| `REMAIN` | whole seconds left in the active phase (rounded up; 0 if idle)|

Example:

```
STATUS STATE=RED RUN=1 R=30 G=20 Y=5 REMAIN=27
```

## State machine

```
        START                +-----------------------------+
IDLE ──────────► RED ──(red_sec)──► GREEN ──(green_sec)──► YELLOW
 ▲                                                            │
 │                               (yellow_sec)                 │
 └──────────────────── STOP ◄────────────────────────────────┘
                     (from any state)
```

- Cycling always starts on **RED**.
- Transitions are driven solely by phase-timer expiry, polled every main-loop
  iteration — there are no blocking delays.
- Changing the duration of the *currently active* phase re-arms its timer from
  the moment of the change; other phases take effect on their next entry.
- `STOP` extinguishes all signal lamps and the RUN indicator and returns to
  `IDLE`.
