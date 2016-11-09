/*
 * Copyright © 2010 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <modbus.h>
//gcc modbus_write_rtu.c -o modbus_write_rtu `pkg-config --libs --cflags libmodbus`
int main(int argc, char *argv[])
{
    modbus_t *ctx;
    struct timeval response_timeout,old_response_timeout;
    int rc=-1;
    int data,addr,type,slaveid;
    if(argc==5)
    {
        slaveid=atoi(argv[1]);
        type=atoi(argv[2]);
        addr=atoi(argv[3])-1;
        data=atoi(argv[4]);
        
        ctx = modbus_new_rtu("/dev/ttyO1", 9600, 'E', 8, 1);
        if (ctx == NULL) {
            fprintf(stderr, "Unable to allocate libmodbus context\n");
            exit(1);
        }
        //modbus_set_debug(ctx, TRUE);
        //modbus_set_error_recovery(ctx,
                                  //MODBUS_ERROR_RECOVERY_LINK |
                                 //MODBUS_ERROR_RECOVERY_PROTOCOL);
        
        
        modbus_set_slave(ctx, slaveid);
        
        modbus_rtu_set_gpio_rts(ctx, 44);
        modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
        
        if (modbus_connect(ctx) == -1) {
            fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            modbus_free(ctx);
            exit(1);
        }
        /* Save original timeout */
        //modbus_get_response_timeout(ctx, &old_response_timeout);
    
        /* Define a new and too short timeout */
        response_timeout.tv_sec = 1;
        response_timeout.tv_usec = 0;
        modbus_set_response_timeout(ctx, &response_timeout);
        switch(type)
        {
            case 0:
            rc =modbus_write_bit(ctx,addr, data);
            if (rc == 1) {
                fprintf(stdout,"%d",data);
            } else {
                fprintf(stderr,"%d",rc);
            }
            break;
            case 4:
            rc =modbus_write_register(ctx,addr, data);
            if (rc == 1) {
                fprintf(stdout,"%d",data);
            } else {
                fprintf(stderr,"%d",rc);
            }
            break;
        }
        /* Close the connection */
        modbus_close(ctx);
        modbus_free(ctx);
    }
    if(rc == 1) exit(0);
    else exit(1);
}
