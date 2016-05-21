#ifndef SHARED_H
#define SHARED_H

#define DEFAULT_SQL_CONN_NAME   "qt_sql_default_connection"
#define NOT_SET                 -1
#define BUILDINGS               1
#define FLOORS                  2
#define ROOMS                   3
#define TYPE                    4
#define PAY_CASH                "Cash"
#define PAY_CHEQUE              "Cheque"
#define PAY_ELECTRONIC          "Electronic"
#define VAL_NICKEL              0.05f
#define VAL_DIME                0.1f
#define VAL_QUARTER             0.25f
#define VAL_LOONIE              1
#define VAL_TOONIE              2
#define VAL_FIVE                5
#define VAL_TEN                 10
#define VAL_TWENTY              20
#define VAL_FIFTY               50
#define VAL_HUNDRED             100

/* REPORT Constants */
// Use date, shiftNo
#define CHECKOUT_REPORT       0
#define VACANCY_REPORT        1
#define LUNCH_REPORT          2
#define WAKEUP_REPORT         3

// Use shiftNo
#define BOOKING_REPORT        4
#define TRANSACTION_REPORT    5
#define CLIENT_REPORT         6
#define OTHER_REPORT          7

// No params
#define YELLOW_REPORT         8
#define RED_REPORT            9

#define NUMCOLS_TRANSACTION   12

#endif // SHARED_H
