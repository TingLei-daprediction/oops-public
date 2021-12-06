#!/usr/bin/env python3
"""
@author: Mayeul Destouches
@author: modified by Benjamin Menetrier for JEDI
@description: plot fields or increments if two paths are specified
"""

import os
import sys
import subprocess
import argparse
import numpy as np
import netCDF4
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker

def func(args):
    """! Plot fields"""

    # Geophysical parameters
    req = 6371229.0                      # Earth radius at equator (m)
    domain_zonal = 2 * np.pi * req       # Model domain in zonal direction (m)
    domain_meridional = 0.5 * np.pi *req # Model domain in meridional direction (m)

    # Variables to plot
    variables = ["x", "q"]

    # File path
    filepaths = []
    ids = []
    if args.gif is None:
        filepaths.append(args.filepath)
    else:
        check_convert = subprocess.getstatusoutput('convert --help')
        if check_convert[0] != 0:
            print("   Error: convert (imagemagick package) should be available to create an animated gif")
            sys.exit(1)
        if "%id%" in args.filepath:
           for id in args.gif.split(","):
               ids.append(id)
               filepaths.append(args.filepath.replace("%id%", id))
        else:
            print("   Error: filepath should include a %id% pattern for gif generation")
            sys.exit(1)

    # Loop over variables
    for variable in variables:
        # Fields vector
        fields = []
        if args.plotwind:
            fields_u = []
            fields_v = []

        # Loop over filepaths
        for filepath in filepaths:
            # Check file extension
            if not filepath.endswith(".nc"):
                print("   Error: filepath extension should be .nc")
                sys.exit(1)

            # Load fields
            fields.append(netCDF4.Dataset(filepath).variables[variable][:])
            if args.plotwind:
                 fields_u.append(netCDF4.Dataset(filepath).variables["u"][:])
                 fields_v.append(netCDF4.Dataset(filepath).variables["v"][:])

        if not args.basefilepath is None:
            # Check file extension
            if not args.basefilepath.endswith(".nc"):
                print("   Error: basefilepath extension should be .nc")
                sys.exit(1)

            # Load base fields
            field_base = netCDF4.Dataset(args.basefilepath).variables[variable][:]
            if args.plotwind:
                field_u_base = netCDF4.Dataset(args.basefilepath).variables["u"][:]
                field_v_base = netCDF4.Dataset(args.basefilepath).variables["v"][:]

            # Compute increments
            for field in fields:
                field = field-field_base
            if args.plotwind:
                for field_u in fields_u:
                    field_u = field_u-field_u_base
                for field_v in fields_v:
                    field_v = field_v-field_v_base

        # Get geometry
        nz, ny, nx = fields[0].shape
        levels = list(range(nz))
        z_coord = netCDF4.Dataset(filepaths[0]).variables["z"][:]
        dx = domain_zonal / nx # zonal grid cell in km
        dy = domain_meridional / (ny + 1) # meridional cell in km
        x_coord = (np.arange(1, nx+1) - 0.5) * dx
        lon_coord = x_coord / domain_zonal * 360 - 180
        y_coord = np.arange(1, ny+1) * dy
        lat_coord = y_coord / domain_meridional * 90
        xx, yy = np.meshgrid(lon_coord, lat_coord)

        # Define color levels
        clevels = []
        for level in levels:
            vmax = 0.0
            for field in fields:
                vmax = max(vmax, np.max(np.abs(field[level])))
            clevels.append(np.linspace(-vmax, vmax, 30))

        # Define plot
        params = {
            "font.size": 12,
            "text.latex.preamble" : r"\usepackage{amsmath}\usepackage{amsfonts}",
            "ytick.left": False,
            "ytick.labelleft": False,
        }
        plt.rcParams.update(params)
        my_formatter = mticker.FuncFormatter(lambda x, pos:"{:.0f}$\degree$E".format(x).replace("-", "\N{MINUS SIGN}"))
        if args.plotwind:
            # Select scale
            if args.basefilepath is None:
                scale = 400
            else:
                scale = 200
            dx_quiver = max(nx//20, 1)
            dy_quiver = max(ny//10, 1)

        # Initialize gif command
        if not args.gif is None:
             cmd = "convert -delay 20 -loop 0 "

        for iplot in range(0, len(filepaths)):
            fig, axs = plt.subplots(nrows=2, figsize=(7,3.5))

            # Loop over levels
            for level, ax in zip(levels, axs[::-1]):
                # Plot variable
                im = ax.contourf(xx, yy, fields[iplot][level], cmap="plasma", levels=clevels[level])

                if args.plotwind:
                    # Plot wind field
                    ax.quiver(xx[::dy_quiver, ::dx_quiver], yy[::dy_quiver, ::dx_quiver],
                                  fields_u[iplot][level, ::dy_quiver, ::dx_quiver], fields_v[iplot][level, ::dy_quiver, ::dx_quiver],
                                  scale=scale, scale_units="inches")

                # Set plot formatting
                ax.set_aspect("equal")
                cb = fig.colorbar(im, ax=ax, shrink=0.9, format=("%.1e" if variable == "q" else None))
                ax.set_ylabel("Altitude {:.0f}$\,$m".format(z_coord[level]))
                ax.xaxis.set_major_formatter(my_formatter)

                # Set title
                varname = dict(x="Streamfunction", q="Potential vorticity").get(variable)
                unit = dict(x="m$^2$s$^{-1}$", q="s$^{-1}$").get(variable)
                fig.suptitle(varname + " in " + unit)
                fig.subplots_adjust(left=0.04, right=0.98, bottom=0.04, top=0.9, hspace=0.01)

            # Save plot
            if args.output is None:
                plotpath = os.path.splitext(os.path.basename(filepaths[iplot]))[0]
            else:
                plotpath = args.output
                if not args.gif is None:
                    plotpath = plotpath.replace("%id%", ids[iplot])
            if args.basefilepath is None:
                plotpath = plotpath + "_" + str(variable) + ".jpg"
            else:
                plotpath = plotpath + "_" + str(variable) + "_incr.jpg"
            if not args.gif is None:
                cmd = cmd + plotpath + " "
                if iplot == 0:
                    gifpath = plotpath.replace(".jpg", ".gif")
            plt.savefig(plotpath, format="jpg", dpi=300)
            plt.close()
            print(" -> plot produced: " + plotpath)

        if not args.gif is None:
            cmd = cmd + gifpath
            os.system(cmd)
            print(" -> gif produced: " + gifpath)
