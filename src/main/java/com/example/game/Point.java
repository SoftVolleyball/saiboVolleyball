package com.example.game;

import com.example.entity.Ball;
import com.example.entity.BallType;
import com.example.entity.Player;
import com.example.group.Lineup;
import com.example.group.Role;
import com.example.group.Team;

import java.util.ArrayList;
import java.util.Random;

public class Point {
    private final Lineup servingTeam;
    private final Lineup receivingTeam;

    private final Random random = new Random();

    public Point(Lineup servingTeam, Lineup receivingTeam) {
        this.servingTeam = servingTeam;
        this.receivingTeam = receivingTeam;
    }

    private Team score() {
        //发球
        Ball ball = servingTeam.getPlayer(1).serve();
        if (ball.distance() < 0 || ball.distance() > 9) return receivingTeam.getTeam();

        Lineup possessionTeam = receivingTeam;
        while (true) {
            //探头球
            if ((ball.type() == BallType.OVER_PASS || ball.type() == BallType.OVER_SET) && ball.distance() < 1) {
                int position = random.nextInt(3);
                Player spiker = possessionTeam.getFront().get(position);
                ball = spiker.spike(ball, position, possessionTeam.getTeam().getRole(spiker));

                if (ball.type() == BallType.UNDER_NET || ball.type() == BallType.OUT_BALL) {
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    return possessionTeam.getTeam();
                }

                possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                continue;
            }

            //拦网
            if (ball.type() == BallType.SPIKE || ball.type() == BallType.ONE_PASS_ATTACK) {
                ArrayList<Player> blockers = new ArrayList<>();
                ArrayList<Player> players = possessionTeam.getFront();
                for (int i = (ball.type() == BallType.ONE_PASS_ATTACK ? 1 : 0); i < 3; i++) {
                    Player player = players.get(i);
                    if (!blockers.contains(player)) blockers.add(player);
                }
                ArrayList<Ball> balls = new ArrayList<>();
                for (Player blocker : blockers) balls.add(blocker.block(ball));
                ball = balls.getFirst();
                if (ball.type() == BallType.OUT_BALL) ball = balls.getLast();
                if (ball.type() == BallType.OUT_BALL) {
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    return possessionTeam.getTeam();
                }
                if (ball.type() == BallType.BLOCK) {
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    continue;
                }
            }

            //第一次触球
            boolean isFrontBall = ball.distance() < 3;
            Player passer;
            if (isFrontBall) {
                ArrayList<Player> front = possessionTeam.getFront();
                front.remove(possessionTeam.getSetter());
                passer = front.get(random.nextInt(front.size()));
            } else {
                ArrayList<Player> back = possessionTeam.getBack();
                back.remove(possessionTeam.getSetter());
                Player libero = null;
                for (Player player : back) if (possessionTeam.getTeam().getRole(player) == Role.L) libero = player;
                if (libero != null) back.add(libero);
                passer = back.get(random.nextInt(back.size()));
            }
            ball = passer.pass(ball);

            //第二次触球
            Player setter = null;
            switch (ball.type()) {
                case FAILED_PASS:
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    return possessionTeam.getTeam();

                case OVER_PASS:
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    continue;

                case BAD_PASS:
                    while (setter == null || setter == passer) setter = possessionTeam.getPlayer(random.nextInt(6) + 1);
                    ball = setter.pass(ball);
                    break;

                case PASS:
                    int position = 0;
                    for (int i = 1; i <= 6; i++) {
                        if (possessionTeam.getPlayer(i) == possessionTeam.getSetter()) {
                            position = i;
                            break;
                        }
                    }
                    ball = possessionTeam.getSetter().set(ball, position);
                    break;

                default:
                    break;
            }

            //第三次触球
            Player spiker = null;
            switch (ball.type()) {
                case FAILED_PASS:
                case UNDER_NET:
                case OUT_BALL:
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    return possessionTeam.getTeam();

                case OVER_PASS:
                case OVER_SET:
                case ONE_PASS_ATTACK:
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    continue;

                case BAD_PASS:
                case BAD_SET:
                    int position = 0;
                    while (spiker == null || spiker == setter) {
                        position = random.nextInt(6) + 1;
                        spiker = possessionTeam.getPlayer(position);
                    }
                    ball = spiker.spike(ball, position, possessionTeam.getTeam().getRole(spiker));
                    break;

                case PASS:
                case SET:
                    int position_ = 0;
                    ArrayList<Player> players = ball.distance() < 3 ? possessionTeam.getFront() : possessionTeam.getBack();
                    while (spiker == null || spiker == setter || possessionTeam.getTeam().getRole(spiker) == Role.L) {
                        position_ = random.nextInt(6) + 1;
                        spiker = possessionTeam.getPlayer(position_);
                    }
                    ball = spiker.spike(ball, position_, possessionTeam.getTeam().getRole(spiker));
                    break;

                default:
                    break;
            }

            switch (ball.type()) {
                case UNDER_NET:
                case OUT_BALL:
                case FAILED_PASS:
                    possessionTeam = (possessionTeam == servingTeam) ? receivingTeam : servingTeam;
                    return possessionTeam.getTeam();
            }
        }
    }
}
