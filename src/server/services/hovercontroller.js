// input a pointer to launchpad
var HoverController = function(launchpad) {
    var launchpad_reference = launchpad;

    var m1 = { roll:0, pitch:0 };
    var m2 = { roll:0, pitch:0 };
    var m3 = { roll:0, pitch:0 };
    var m4 = { roll:0, pitch:0 };

    var motor_pwr = { m1:127, m2:127, m3:127, m4:127 };

    // constants for pd controller
    var roll_state = { pk:10, dk:1 };
    var pitch_state = { pk:10, dk:1 };
    var yaw_state = { pk:10, dk:1 };

    var update_controller = function( estimated_roll, estimated_pitch ) {
        update_roll( estimated_roll );
        update_pitch( estimated_pitch );
        update_total();
        update_motor();
    };

    // counteract the estimated roll perturbation with a roll in the opposite direction
    var update_roll = function( estimated_roll ) {
        var roll_power_change = (estimated_roll * roll_state.pk);
        m1.roll += roll_power_change;
        m2.roll -= roll_power_change;
        m3.roll -= roll_power_change;
        m4.roll += roll_power_change;
    };

    // counteract the estimated pitch perturbation with a pitch in the opposite direction
    var update_pitch = function( estimated_pitch ) {
        var diff_pitch = desired_pitch - estimate_pitch;
        pitch_state.isum += diff_pitch;
        var pitch_power_change = (estimated_pitch * pitch_state.pk);
        m1.pitch += pitch_power_change;
        m2.pitch -= pitch_power_change;
        m3.pitch += pitch_power_change;
        m4.pitch -= pitch_power_change;
    };

    // update total power
    var update_total = function() {
        motor_pwr.m1 += ( m1.pitch + m1.roll );
        motor_pwr.m2 += ( m2.pitch + m2.roll );
        motor_pwr.m3 += ( m3.pitch + m3.roll );
        motor_pwr.m4 += ( m4.pitch + m4.roll );
    };

    // write result to motor
    var update_motor = function() {
        launchpad_reference.write(255);
        launchpad_reference.write(motor_pwr.m1);
        launchpad_reference.write(motor_pwr.m2);
        launchpad_reference.write(motor_pwr.m3);
        launchpad_reference.write(motor_pwr.m4);
    }
};

module.exports = HoverController;